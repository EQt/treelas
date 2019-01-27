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
    nums[end-2] = zero(T)
    nums[end-1] = +one(T)
    nums[end-0] = -one(T)
    nums
end

nums = generate(UInt64, 9)
expect = leading_zeros.(nums)

nums32 = generate(UInt32, 8)
# show(reinterpret.(Int32, (generate(UInt32, 8, 0)))
