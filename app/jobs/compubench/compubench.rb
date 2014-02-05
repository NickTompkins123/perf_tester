require './app/jobs/benchmark_job'

class CompuBenchJob < BenchmarkJob

  SUITES = [
      'Face detection',
      'TV-L1 Optical Flow',
      'Ocean Surface Simulation',
      'Particle Simulation Mobile - 32k',
      'Particle Simulation Mobile - 4k',
      'Particle Simulation Desktop - 64k',
      'Julia Set',
      'Ambient Occlusion',
      'Bitcoin mining',
      'T-Rex',
      'Provence',
      'Denoise Desktop',
      'Denoise Mobile',
      'Separated Gaussian Blur - Global memory usage'
  ]

  def setup_env
    super
    ENV['PATH'] = ENV['PATH'] + ';' + File.join(revision.get_path, 'bin/x86/')
  end

  def execute_benchmark
    script_dir = File.dirname(__FILE__)
    `#{script_dir}/run.exe`
  end

  def parse_and_save_results_to_db
    script_dir = File.dirname(__FILE__)
    cnt = 0
    Dir["#{script_dir}/output/*.csv"].sort_by{ |f| File.mtime(f) }.reverse_each do |filename|
      # Ensure there is no more than one results file
      if cnt >= 1
        File.delete(filename)
        next
      end
      cnt += 1

      File.open(filename) do |file|
        content = file.read

        SUITES.each do |s|
          val = content[/#{s};OK;(\d+[.]?\d*)/, 1].to_f
          Measurement.create(
              suite: Suite.get_ancestor_of(s, suite),
              revision: revision,
              value: val)
        end
      end

      File.delete(filename)
    end
  end

  def self.interpret_compare_result(percentage, suite)
    if suite.name == 'Ocean Surface Simulation'
      percentage = -percentage
    end

    self.superclass.interpret_compare_result(percentage, suite)
  end
end