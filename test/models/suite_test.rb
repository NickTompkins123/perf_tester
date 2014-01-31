require 'test_helper'

class SuiteTest < ActiveSupport::TestCase
  test "get all top level suites" do
    assert_equal 7, Suite.get_top_level.count
  end

  test "ancestor of is working" do
    assert suites("shoc32-fft-DP-FFT-INV_AMD").ancestor_of?(suites("shoc32"))
    assert suites("shoc32-fft-DP-FFT-INV_AMD").ancestor_of?(suites("shoc32-fft"))
    assert suites("shoc32-fft").ancestor_of?(suites("shoc32"))
  end

  test "get ancestor is working" do
    s1 = Suite.where(name: "DP-FFT-INV_AMD")
    s = Suite.get_ancestor_of("DP-FFT-INV_AMD", suites(:shoc32))
    assert_equal suites("shoc32-fft-DP-FFT-INV_AMD"), s
  end
end
