require './app/jobs/dummy/dummy'
require './app/jobs/shoc/shoc'
require './app/jobs/compubench/compubench'
require './app/jobs/basemarkcl/basemarkcl'

Delayed::Worker.max_attempts = 3
PerfTester::Application.config.revision_path = 'H:/Users/ilaevsky/dists/'