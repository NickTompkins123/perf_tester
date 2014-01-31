class CreateRevisions < ActiveRecord::Migration
  def change
    create_table :revisions do |t|
      t.string :name
      t.string :path

      t.timestamps
    end
  end
end
