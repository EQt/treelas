readme.txt: johnson.zip
	unzip $<

johnson.zip:
	wget -O $@ https://www.tandfonline.com/doi/suppl/10.1080/10618600.2012.681238/suppl_file/ucgs_a_681238_sm0876.zip

clean:
	rm -fr johnson.zip c r scripts lib load.r readme.txt
