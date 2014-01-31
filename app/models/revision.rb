class Revision < ActiveRecord::Base
  has_many :measurements

  validates :name, :path, presence: true

  validate :path_should_exist

  def path_should_exist
    unless File.directory?(File.join(self.path, "bin/x86/")) and
           File.directory?(File.join(self.path, "bin/x86_64/")) and
           File.file?(File.join(self.path, "bin/x86/amdhsacl.dll"))
      errors.add(:path, "non existent or invalid dist")
    end
  end
end
