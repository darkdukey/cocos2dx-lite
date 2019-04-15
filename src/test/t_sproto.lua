local sproto = require "sproto"
local parser = require 'sprotoparser'

local schema = [[
  .Person {
    name 0 : string
    id 1 : integer
    email 2 : string

    .PhoneNumber {
        number 0 : string
        type 1 : integer
    }

    phone 3 : *PhoneNumber
}

.AddressBook {
    person 0 : *Person
}
]]

local proto = parser.parse(schema)
sproto.new(proto)

