all:
	cd client && make
	cd seller && make
	mkdir -p ODP
	ln -fs ../client/Client ODP/Client
	ln -fs ../seller/Seller ODP/Seller
