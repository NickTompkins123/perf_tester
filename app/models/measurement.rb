class Measurement < ActiveRecord::Base
  belongs_to :suite
  belongs_to :revision
end
