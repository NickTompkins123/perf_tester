class AddEnviornmentToRevision < ActiveRecord::Migration
  def change
    add_column :revisions, :env, :string, default: "GPU_HSAIL_ENABLE=1"
  end
end
