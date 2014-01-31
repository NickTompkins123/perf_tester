class CreateSuites < ActiveRecord::Migration
  def change
    create_table :suites do |t|
      t.string :name
      t.text :description
      t.references :parent_suite

      t.timestamps
    end
  end
end
