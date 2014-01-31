class CreateMeasurements < ActiveRecord::Migration
  def change
    create_table :measurements do |t|
      t.references :suite, index: true
      t.references :revision, index: true
      t.float :value

      t.timestamps
    end
  end
end
