require './app/jobs/benchmark_job'
require 'nokogiri'

class BasemarkClJob < BenchmarkJob

  SUITES = [
    'Soft Body Test',
    'Fluid Operations Test',
    'Smoothed Particle Hydrodynamics Test',
    'Wave Simulation Test',

    'Mandelbulb Rendering Test',
    'Julia Rendering Test',
    'Online Compiler Test',

    'Image Blur Test',
    'Image Color Correction Test',
    'Image Noise Reduction Test',
    'Image Surface Smoothing Test',
    'Image Sharpening Test',

    'Video Blur Test',
    'Video Color Correction Test',
    'Video Noise Reduction Test',
    'Video Surface Smoothing Test',
    'Video Sharpening Test',
  ]

  def setup_env
    super
    ENV['PATH'] = ENV['PATH'] + ';' + File.join(revision.get_path, 'bin/x86/')
  end

  def execute_benchmark
    script_dir = File.dirname(__FILE__)

    Dir.chdir("#{script_dir}/bench/") do |_|
      `#{script_dir}/bench/basemarkcl_executable.exe`
    end
  end

  def parse_and_save_results_to_db
    script_dir = File.dirname(__FILE__)
    cnt = 0
    Dir["#{script_dir}/bench/reports/*.xml"].sort_by{ |f| File.mtime(f) }.reverse_each do |filename|
      # Ensure there is no more than one results file
      if cnt >= 1
        File.delete(filename)
        next
      end
      cnt += 1

      f = File.open(filename)
      doc = Nokogiri::XML(f)
      f.close

      SUITES.each do |s|
        suite_nodes = doc.xpath("//benchmark/tests/test[@name=\"#{s}\"]/score")
        # TODO: How to do assert in ruby?
        # assert suite_nodes.length == 1

        val = suite_nodes.first.text.to_f
        Measurement.create(
            suite: Suite.get_ancestor_of(s, suite),
            revision: revision,
            value: val)
      end

      File.delete(filename)
    end
  end
end