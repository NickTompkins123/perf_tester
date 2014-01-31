require './app/jobs/benchmark_job'

class BaseShocBenchmarkJob < BenchmarkJob
  SUITES = {
      'FFT' => ['DP-FFT-INV_AMD', 'DP-FFT_AMD', 'SP-FFT-INV_AMD', 'SP-FFT_AMD'],
      'MD' => ['MD-LJ-DP_AMD', 'MD-LJ_AMD'],
      'SGEMM' => ['DGEMM-N_AMD', 'DGEMM-T_AMD', 'SGEMM-N_AMD', 'SGEMM-T_AMD'],
      'Sort' => ['Sort-Rate_AMD'],
      'Spmv' => ['CSR-Vector-DP_AMD', 'CSR-Vector-SP_AMD', 'ELLPACKR-DP_AMD', 'ELLPACKR-SP_AMD'],
      'Stencil2D' => ['DP_Sten2D_AMD', 'SP_Sten2D_AMD']
  }

  def get_arch
    raise "This method should be overridden"
  end

  def execute_benchmark
    script_dir = File.dirname(__FILE__)

    SUITES.each do |command, sub_suites|
      output = `#{script_dir}/bin/#{get_arch}/#{command} -s 3`

      sub_suites.each do |s|
        # Find string with sub suite name, skip all non dots,
        # read first floating point number preceded by space symbol
        val = output[/#{s}[^.]*\s+(\d+[.]\d+)/, 1].to_f
        Measurement.create(
          suite: Suite.get_ancestor_of(s, suite),
          revision: revision,
          value: val)
      end
    end
  end

  def parse_and_save_results_to_db
  end
end

class Shoc32BenchmarkJob < BaseShocBenchmarkJob
  def get_arch
    '32'
  end

  def setup_env
    super
    ENV['PATH'] = ENV['PATH'] + ';' + File.join(revision.path, 'bin/x86/')
  end
end

class Shoc64BenchmarkJob < BaseShocBenchmarkJob
  def get_arch
    '64'
  end

  def setup_env
    super
    ENV['PATH'] = ENV['PATH'] + ';' + File.join(revision.path, 'bin/x86_64/')
  end
end

