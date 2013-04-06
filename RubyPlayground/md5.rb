#!/usr/bin/env ruby
require "rubygems"
require "digest/md5"
require "awesome_print"

# sample inputs
samples = [
	"one",
	"Sarah Kerrigan",
	"Queen of Blades",
	"StarCraft II: Heart of the Swarm",
	"Hatcher => Larva => [{ Drone => [Hatcher, Pool, Roach Warren]}, Zergling, Roach, Hydra, Infestor]"
]

# create an MD5 digest
ruby_md5s = samples.map{ | e | Digest::MD5.hexdigest( e ) }

# now implement it, and see if we come up with the same answer
# ported over from python:
# http://rosettacode.org/wiki/MD5/Implementation#Python
rotate_amounts = [ 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                   5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                   4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                   6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 ]

constants = []
64.times { | i | constants << (Math.sin( i + 1 ).abs * ( 2 ** 32 )).floor }

initial_values = [0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476]

def left_rotate(x, amount)
	x &= 0xFFFFFFFF
	return (( x << amount ) | (x >> (32 - amount ))) & 0xFFFFFFFF
end

# based off http://en.wikipedia.org/wiki/MD5
# and http://rosettacode.org/wiki/MD5/Implementation#Python
def md5(msg)
	# Pre-processing: adding a single 1 bit
	msg + 0b1

	# Pre-processing: padding with zeros
	while ((msg.bytesize * 8) % 512) < 448
		msg += 0b0
	end

	# Process the message in successive 512-bit chunks
	msg.split("").each_slice( 64 ) { |bit_chunk_512|
		# 512_bit_chunk is an arary of characters (64, if ascii or utf-8, utf-16 would be 32 chars..)
		# lets not worry about encoding right now

		# break chunk into 32 bitwords (4 is bytes)
		m = []
		bit_chunk_512.each_slice( 4 ).each_slice { | bit_chunk_32 |
			m << bit_chunk_32
		}

		a, b, c, d = initial_values

		# main loop
		64.times { |i|
			# init vars for scope o' loop
			f = 0
			g = 0

			if 0 <= i <= 15
				f = (b and c) or (!b and d)
				g = i
			elsif 16 <= i <= 31
				f = (d and b) or (!d and c)
				g = (5 * i + 1) % 16
			elsif 32 <= i <= 47
				f = b ^ c ^ d
				g = (3 * i + 5) % 16
			elsif 48 <= i <= 63
				f = c ^ (b or !d)
				g = (7 * i)  % 16
			end

			d_temp = d
			d = c
			c = b
			b = b + left_rotate((a +f + constants[i] + m[g]), rotate_amounts[i])
			a = d_temp

		}

		# Add this chunk's hash to the result so far
		initial_values = [a, b, c, d].zip(initial_values).map { | x | x.inject(:+) }
	}

	result = initial_values.join
	return result
end


my_md5s = samples.map{ | e | md5( e ) }
puts "Actual MD5s:"
ap ruby_md5s
puts "my md5s:"
ap my_md5s

