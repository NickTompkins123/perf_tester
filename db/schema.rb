# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 20140127203024) do

  create_table "delayed_jobs", force: true do |t|
    t.integer  "priority",   default: 0, null: false
    t.integer  "attempts",   default: 0, null: false
    t.text     "handler",                null: false
    t.text     "last_error"
    t.datetime "run_at"
    t.datetime "locked_at"
    t.datetime "failed_at"
    t.string   "locked_by"
    t.string   "queue"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  add_index "delayed_jobs", ["priority", "run_at"], name: "delayed_jobs_priority"

  create_table "measurements", force: true do |t|
    t.integer  "suite_id"
    t.integer  "revision_id"
    t.float    "value"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  add_index "measurements", ["revision_id"], name: "index_measurements_on_revision_id"
  add_index "measurements", ["suite_id"], name: "index_measurements_on_suite_id"

  create_table "revisions", force: true do |t|
    t.string   "name"
    t.string   "path"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.text     "description"
    t.string   "env",         default: "HSA_RUNTIME=1;"
  end

  create_table "suites", force: true do |t|
    t.string   "name"
    t.text     "description"
    t.integer  "parent_suite_id"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "job_class"
  end

end
