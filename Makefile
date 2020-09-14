all:
	cd client && make
	cd seller && make
	cd distributor && make
	mkdir -p ODP
	ln -fs ../client/Client ODP/Client
	ln -fs ../distributor/Distributor ODP/Distributor
	ln -fs ../seller/Seller ODP/Seller
