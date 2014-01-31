class AddDescriptionToRevisions < ActiveRecord::Migration
  def change
    add_column :revisions, :description, :text
  end
end
