class AddEnviornmentToRevision < ActiveRecord::Migration
  def change
    add_column :revisions, :env, :string, default: "HSA_RUNTIME=1;"
  end
end
