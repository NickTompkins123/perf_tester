require './app/jobs/benchmark_job'

class DummyBenchmarkJob < BenchmarkJob
  def execute_benchmark
    script_dir = File.dirname(__FILE__)
    @raw_result = `#{script_dir}/bin/dummy.exe`
  end

  def parse_and_save_results_to_db
    @raw_result.lines.map(&:chomp).each do |line|
      suite_name, value = line.split(/ - /)

      Measurement.create(
          suite: Suite.find_by(name: suite_name),
          revision: revision,
          value: value)
    end
  end
end