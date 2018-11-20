local current = redis.call('GET', KEYS[1])
if current ~= false then
    return true
end

if current == false then
    redis.call('SET', KEYS[1], ARGV[1])
end

redis.call('expire', KEYS[1], 10);

return false