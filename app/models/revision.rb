class Revision < ActiveRecord::Base
  has_many :measurements

  validates :name, :path, presence: true

  validate :path_should_exist

  def path_should_exist
    unless File.directory?(File.join(self.get_path, "bin/x86/")) and
           File.directory?(File.join(self.get_path, "bin/x86_64/")) and
           File.file?(File.join(self.get_path, "bin/x86/amdhsacl.dll"))
      errors.add(:path, "is non existent or dist is invalid")
    end
  end

  def get_path
    File.join(PerfTester::Application.config.revision_path, self.path, 'windows', 'debug')
  end
end
