class BenchmarkJob < Struct.new(:revision, :suite)
  def perform
    setup_env
    execute_benchmark
    parse_and_save_results_to_db
    clean_env
  end

  # Returns hash {suite => [lhs_val, rhs_val, percentage, good_or_bad]}
  # good_or_bad = +1 if result is progression
  #                0 no change
  #               -1 if result is regression
  def self.compare(lhs_rev, rhs_rev, suite)
    ret = {}
    lhs = Measurement.find_by(revision: lhs_rev, suite: suite)
    rhs = Measurement.find_by(revision: rhs_rev, suite: suite)
    if lhs and rhs and lhs.value != 0 and rhs.value != 0
      val = compare_measurements(lhs, rhs)
      ret[suite] = [lhs.value, rhs.value, val, interpret_compare_result(val, suite)]
    end

    suite.sub_suites.each do |sub_suite|
      ret.merge!(self.compare(lhs_rev, rhs_rev, sub_suite))
    end

    return ret
  end

protected
  def self.compare_measurements(lhs, rhs)
    ((rhs.value - lhs.value) / lhs.value) * 100
  end

  def self.interpret_compare_result(percentage, suite)
    if percentage >= 5 then 1 elsif percentage <= -5 then -1 else 0 end
  end

  def setup_env
    require 'shellwords'

    @new_env_vars = []
    revision.env.split(/;/).map{|t| t.strip}.each do |assignment|
      if assignment
        name, *val = assignment.split(/=/).map{|t| t.strip}
        # In case if equality sign was used inside value
        # e.g T="Hello=world"
        val = val.join('=')

        name = Shellwords.shellwords(name)[0]
        val = Shellwords.shellwords(val)[0]

        ENV[name] = val
        @new_env_vars += [name]
      end
    end
  end

  def execute_benchmark
  end

  def parse_and_save_results_to_db
  end

  def clean_env
    @new_env_vars.each do |name|
      ENV[name] = nil
    end

    ENV['PATH'] = ENV['PATH'].split(/;/)[0...-1].join(';')
  end
end

