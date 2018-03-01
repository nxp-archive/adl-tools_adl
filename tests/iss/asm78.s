#
# Test to make sure that we detect when too large of a relocation is used.
# In this case, 'l' is OK, but 'b' is too big and will cause an error.
#
	lwz r1,L2@l(r0)
	lwz r1,L2@b(r0)
