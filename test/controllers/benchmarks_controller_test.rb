require 'test_helper'

class BenchmarksControllerTest < ActionController::TestCase
  test "should route to revisions list" do
    assert_routing "/", {controller: 'benchmarks', action: 'revision_list'}
  end

  test "should get revisions list" do
    get :revision_list
    assert_response :success
    assert_not_nil assigns(:revisions)
  end

  test "should route to measurements list" do
    assert_routing "measurements/42", {controller: 'benchmarks', action: 'measurements_list', id: '42'}
  end

  test "should get measurements list for revision" do
    get :measurements_list, id: revisions(:scnd).id
    assert_response :success

    assert_not_nil assigns(:revision)
    assert_equal '920001', assigns(:revision).name
  end

  test "should route to new revision" do
    assert_routing "start", {controller: 'benchmarks', action: 'new_revision'}
  end

  test "should route to run benchmarks" do
    assert_routing({method: 'post', path: "start"},
                   {controller: 'benchmarks', action: 'run_benchmarks'})
  end

  test "new revision validates name and path" do
    post :run_benchmarks, revision: {name: '',
                                     path: ''}
    assert_template "new_revision"
    assert_not_nil assigns(:revision)
    assert assigns(:revision).errors.any?
  end

  test "compare revisions controller" do
    get :revision_compare, {left_id: revisions(:fst).id, right_id: revisions(:scnd).id}
    assert_response :success

    assert_not_nil assigns(:compare_results)
    assert_equal 5, assigns(:num_tests_run)
  end

  test "new revision was added and benchmarks were run" do
    old_val = Delayed::Worker.delay_jobs
    Delayed::Worker.delay_jobs = false

    old_count = Measurement.count
    # Check that revision is added
#    assert_difference('Revision.count') do
#      post :run_benchmarks, revision: {name: 'Test running new revision',
#                                       path: 'No path',
#                                       description: 'testing'}
#    end

    # Check that there is any difference in amount of measurements
    # Which means at least some benchmarks were run
    assert_not_equal old_count, Measurement.count

    assert_not_nil assigns(:revision)
    assert_redirected_to({controller: 'benchmarks',
                          action: 'measurements_list',
                          id: assigns(:revision).id.to_s})

    Delayed::Worker.delay_jobs = old_val
  end
end
