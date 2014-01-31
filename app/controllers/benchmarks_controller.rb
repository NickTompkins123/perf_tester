require 'pp'

class BenchmarksController < ApplicationController
  def revision_list
    @revisions = Revision.order(created_at: :desc)
  end

  def measurements_list
    @revision = Revision.find(params[:id].to_i)
  end

  def new_revision
    @revision = Revision.new
  end

  def run_benchmarks
    # Add new revision
    @revision = Revision.create(get_revision_params)

    if @revision.errors.any?
      render "new_revision"
    else
      # Schedule all available suites
      Suite.all.each do |s|
        unless s.job_class.nil?
          Delayed::Job.enqueue s.job_class.constantize.new(@revision, s)
        end
      end
      redirect_to({controller: 'benchmarks', action: 'measurements_list', id: @revision.id.to_s},
                   notice: "Revision was successfully created")
    end
  end

  def select_right_side_for_compare
    revision_list
    @left_side_revision = Revision.find(params[:id].to_i)
    render "revision_list"
  end

  def revision_compare
    @lhs_revision = Revision.find(params[:left_id].to_i)
    @rhs_revision = Revision.find(params[:right_id].to_i)

    # Get compare results
    @compare_results = {}
    Suite.all.each do |s|
      unless s.job_class.nil?
        @compare_results.merge!(s.job_class.constantize.compare(@lhs_revision, @rhs_revision, s)) do |_, _, _|
          assert false # There should not be any intersecting suites
        end
      end
    end

    # Collect summary
    @num_progressions = 0
    @num_regressions = 0
    @num_tests_run = 0
    @compare_results.each do |(k, v)|
      @num_tests_run  += 1
      if v[3] > 0
        @num_progressions += 1
      elsif v[3] < 0
        @num_regressions += 1
      end
    end
  end

private
  def get_revision_params
    params.require(:revision).permit(:name, :path, :description, :env)
  end
end
