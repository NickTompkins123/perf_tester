require 'test_helper'

class JobTest < ActionController::TestCase
  def setup
    @old_delay_jobs = Delayed::Worker.delay_jobs
    Delayed::Worker.delay_jobs = false
  end

  def teardown
    Delayed::Worker.delay_jobs = @old_delay_jobs
  end

  test "job correctly setups and cleans env" do
    b = Shoc32BenchmarkJob.new(revisions(:fst), suites(:shoc32))

    b.send(:setup_env) # it is protected method, bypass that fact
    assert_equal ENV['HSA_RUNTIME'], '1'
    assert_equal ENV['SOME_VAR'], 'Hello world'
    assert_not_nil ENV['PATH'].split(/;/).index('C:/Projects/dist.920000/bin/x86/')

    b.send(:clean_env)
    assert_nil ENV['HSA_RUNTIME']
    assert_nil ENV['SOME_VAR']
    assert_nil ENV['PATH'].split(/;/).index('C:/Projects/dist.920000/bin/x86/')
  end

  test "results compare" do
    res = Shoc32BenchmarkJob.compare(revisions(:fst), revisions(:scnd), suites(:suite1))
    assert_equal 4, res.length
  end

  test "dummy benchmark" do
    new_rev = Revision.create(name: 'DUMMY_TEST',
                              path: 'no path',
                              description: 'testing dummy benchmark job')

    Delayed::Job.enqueue DummyBenchmarkJob.new(new_rev, suites(:suite1))

    expected_results = {sub_suite11: 42, sub_suite12: 43,
                        sub_suite13: 44, sub_suite14: 45, suite2: 47}
    expected_results.each do |suite_name, expected_val|
      m = suites(suite_name).measurements.where(revision: new_rev)
      assert_equal 1, m.count
      assert_equal expected_val, m.take.value
    end
  end

  test "shoc benchmark job" do
    new_rev = Revision.create(name: 'SHOC_TEST',
                              path: 'no path',
                              description: 'testing shoc benchmark job')

    old_count = Measurement.count
    Delayed::Job.enqueue Shoc32BenchmarkJob.new(new_rev, suites(:shoc32))
    Delayed::Job.enqueue Shoc64BenchmarkJob.new(new_rev, suites(:shoc64))
    assert_not_equal old_count, Measurement.count

    # Check that there is at least one non nil measurement
    m = Measurement.where(revision: new_rev,
                          suite: suites("shoc32-fft-DP-FFT-INV_AMD")).take
    assert_not_nil m
    assert_not_equal 0, m.value
  end

  test "compubench" do
    new_rev = Revision.create(name: 'COMPUBENCH_TEST',
                              path: 'no path',
                              description: 'testing compubench job')

    old_count = Measurement.count
    Delayed::Job.enqueue CompuBenchJob.new(new_rev, suites(:compubench))
    assert_not_equal old_count, Measurement.count

    # Check that there is at least one non nil measurement
    m = Measurement.where(revision: new_rev,
                          suite: suites("compubench-facedetect")).take
    assert_not_nil m
    assert_not_equal 0, m.value
    end

  test "basemarkcl" do
    new_rev = Revision.create(name: 'BASEMARK_TEST',
                              path: 'no path',
                              description: 'testing basemark job')

    old_count = Measurement.count
    Delayed::Job.enqueue BasemarkClJob.new(new_rev, suites(:basemark))
    assert_not_equal old_count, Measurement.count

    # Check that there is at least one non nil measurement
    m = Measurement.where(revision: new_rev,
                          suite: suites("basemark-softbody")).take
    assert_not_nil m
    assert_not_equal 0, m.value
  end
end