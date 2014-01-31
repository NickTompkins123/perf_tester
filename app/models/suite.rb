class Suite < ActiveRecord::Base
  has_many :sub_suites, class_name: "Suite", foreign_key: "parent_suite_id"
  belongs_to :parent_suite, class_name: "Suite"

  has_many :measurements

  validates :job_class, inclusion: { in: %w(DummyBenchmarkJob Shoc32BenchmarkJob Shoc64BenchmarkJob
                                            CompuBenchJob BasemarkClJob),
                                     message: "%{value} is not a valid suite type",
                                     allow_nil: true}

  def get_measurement_for_revision(revision)
    self.measurements.find_by(revision: revision)
  end

  def self.get_top_level
    Suite.where(parent_suite_id: nil)
  end

  def ancestor_of?(root)
    s = self
    while s != nil
      if s == root
        return true
      end
      s = s.parent_suite
    end

    return false
  end

  def self.get_ancestor_of(name, root)
    # TODO: Why all.each does not work? (parent_suite always nil)
    where(name: name).each do |s|
      if s.ancestor_of?(root)
        return s
      end
    end

    return nil
  end
end
