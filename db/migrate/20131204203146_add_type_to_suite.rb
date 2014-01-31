class AddTypeToSuite < ActiveRecord::Migration
  def change
    add_column :suites, :job_class, :string
  end
end
