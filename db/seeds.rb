# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rake db:seed (or created alongside the db with db:setup).
#
# Examples:
#
#   cities = City.create([{ name: 'Chicago' }, { name: 'Copenhagen' }])
#   Mayor.create(name: 'Emanuel', city: cities.first)

###
# Fill shoc benchmark
###
['32', '64'].each do |platform|
  root = Suite.create!(name: "SHOC #{platform}",
                       job_class: "Shoc#{platform}BenchmarkJob")

  BaseShocBenchmarkJob::SUITES.each do |first_child_name, sub_childes|
    first_child = Suite.create!(name: first_child_name,
                                parent_suite: root)

    sub_childes.each do |s_name|
      Suite.create!(name: s_name,
                    parent_suite: first_child)
    end
  end
end

###
# Fill compubench
###
root = Suite.create!(name: "CompuBench",
                     job_class: "CompuBenchJob")

CompuBenchJob::SUITES.each do |suite|
  Suite.create!(name: suite,
                parent_suite: root)
end

###
# Fill basemarkcl
###
root = Suite.create!(name: "BasemarkCL",
                     job_class: "BasemarkClJob")

BasemarkClJob::SUITES.each do |suite|
  Suite.create!(name: suite,
                parent_suite: root)
end
