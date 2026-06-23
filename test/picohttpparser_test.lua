pcall(require, 'luacov')
local testcase = require('testcase')
local assert = require('assert')
local picohttpparser = require('picohttpparser')

local function new_parser()
    return picohttpparser.new()
end

function testcase.new_returns_object()
    local phr, err = picohttpparser.new()
    assert.is_nil(err)
    assert.not_nil(phr)
end

function testcase.new_with_maxheader()
    local phr = picohttpparser.new(64)
    assert.not_nil(phr)
end

function testcase.new_rejects_maxheader_above_uint8_max()
    -- maxhdr is stored as uint8_t; values > 255 must be rejected
    local ok, err = pcall(picohttpparser.new, 256)
    assert.is_false(ok)
    assert.match(err, 'maxhdr must be less than UINT8_MAX', false)
end

function testcase.new_reports_calloc_failure_on_negative_maxheader()
    -- negative maxhdr becomes a huge size_t, calloc must fail
    local phr, err = picohttpparser.new(-1)
    assert.is_nil(phr)
    assert.is_string(err)
end

function testcase.tostring_renders_metatable_tag()
    local phr = new_parser()
    assert.match(tostring(phr), '^picohttpparser: 0x', false)
end

function testcase.parse_request_parses_simple_get()
    local phr = new_parser()
    local req = {}
    local hdr = {}
    local body = 'GET /foo HTTP/1.1\r\nHost: example.com\r\n\r\n'
    local consumed = phr:parseRequest(req, hdr, body)
    assert.is_number(consumed)
    assert.equal(consumed, #body)
    assert.equal(req.method, 'GET')
    assert.equal(req.path, '/foo')
    assert.equal(req.minor_version, 1)
    assert.equal(hdr['Host'], 'example.com')
end

function testcase.parse_request_lowercases_header_names_when_asked()
    local phr = new_parser()
    local req = {}
    local hdr = {}
    local body = 'GET /foo HTTP/1.1\r\nHost: example.com\r\n\r\n'
    phr:parseRequest(req, hdr, body, 0, true)
    assert.equal(hdr['host'], 'example.com')
end

function testcase.parse_request_reports_partial_input()
    local phr = new_parser()
    local req = {}
    local hdr = {}
    local partial = 'GET /foo HTTP/1.1\r\nHost: example.com\r\n'
    local consumed = phr:parseRequest(req, hdr, partial)
    -- incomplete request: parser returns -2 to signal more data is needed
    assert.equal(consumed, -2)
end

function testcase.parse_response_parses_simple_ok()
    local phr = new_parser()
    local res = {}
    local hdr = {}
    local body = 'HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n'
    local consumed = phr:parseResponse(res, hdr, body)
    assert.is_number(consumed)
    assert.equal(consumed, #body)
    assert.equal(res.minor_version, 1)
    assert.equal(res.status, 200)
    assert.equal(res.message, 'OK')
    assert.equal(hdr['Content-Type'], 'text/plain')
end

function testcase.parse_response_lowercases_header_names_when_asked()
    local phr = new_parser()
    local res = {}
    local hdr = {}
    local body = 'HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n'
    phr:parseResponse(res, hdr, body, 0, true)
    assert.equal(hdr['content-type'], 'text/plain')
end

function testcase.parse_response_reports_partial_input()
    local phr = new_parser()
    local res = {}
    local hdr = {}
    local partial = 'HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n'
    local consumed = phr:parseResponse(res, hdr, partial)
    assert.equal(consumed, -2)
end
