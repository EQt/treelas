# generate examples with known number of leading zeros
import Random: MersenneTwister

function generate(T = UInt64, n = 9, seed = 0)
    nums = rand(MersenneTwister(seed), T, n)
    bitlen = 8 * sizeof(T)
    ONE = one(T) << (bitlen - 1)
    @assert bitstring(ONE)[1] == '1'
    @assert all(bitstring(ONE)[i] == '0' for i=2:bitlen)
    nums .|= ONE    # all start by ONE
    for i in 1:n
        nums[i] >>= 2i^2 - 5i + 3
    end
    nums[7] = +one(T)
    nums[8] = -one(T)
    nums
end

nums = generate(UInt64, 9)
expect = leading_zeros.(nums)

nums32 = generate(UInt32, 8)
