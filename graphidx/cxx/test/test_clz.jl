# generate examples with known number of leading zeros
import Random: MersenneTwister

n = 9
nums = rand(MersenneTwister(0), UInt64, n)
ONE = UInt64(1) << 63
@assert bitstring(ONE)[1] == '1'
@assert all(bitstring(ONE)[i] == '0' for i=2:64)
nums .|= ONE    # all start by ONE
for i in 1:n
    nums[i] >>= 2i^2 - 5i + 3
end
nums[7] = 1
nums[8] = reinterpret(UInt64, -1)

expect = leading_zeros.(nums)

