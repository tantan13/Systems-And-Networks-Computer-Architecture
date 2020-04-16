# -*- mode: ruby -*-
# vi: set ft=ruby :

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure("2") do |config|
    
    # The most common configuration options are documented and commented below.
    # For a complete reference, please see the online documentation at
    # https://docs.vagrantup.com.
    
    # Official Ubuntu 16.04 base box
    config.vm.box = "ubuntu/bionic64"
    
    # Set the name of the box
    config.vm.define "cs2200"
    
    # Disable automatic box update checking. If you disable this, then
    # boxes will only be checked for updates when the user runs
    # `vagrant box outdated`. This is not recommended.
    # config.vm.box_check_update = false
    
    # Create a forwarded port mapping which allows access to a specific port
    # within the machine from a port on the host machine. In the example below,
    # accessing "localhost:8080" will access port 80 on the guest machine.
    # NOTE: This will enable public access to the opened port
    # config.vm.network "forwarded_port", guest: 80, host: 8080
    
    # Create a forwarded port mapping which allows access to a specific port
    # within the machine from a port on the host machine and only allow access
    # via 127.0.0.1 to disable public access
    # config.vm.network "forwarded_port", guest: 80, host: 8080, host_ip: "127.0.0.1"
    
    # Create a private network, which allows host-only access to the machine
    # using a specific IP.
    # config.vm.network "private_network", ip: "192.168.33.10"
    
    # Create a public network, which generally matched to bridged network.
    # Bridged networks make the machine appear as another physical device on
    # your network.
    # config.vm.network "public_network"
    
    # Share an additional folder to the guest VM. The first argument is
    # the path on the host to the actual folder. The second argument is
    # the path on the guest to mount the folder. And the optional third
    # argument is a set of non-required options.
    config.vm.synced_folder "./vagrant", "/vagrant"
    
    # Provider-specific configuration so you can fine-tune various
    # backing providers for Vagrant. These expose provider-specific options.
    #
    config.vm.provider "virtualbox" do |vb|
        vb.name = "cs2200"
        
        # Display the VirtualBox GUI when booting the machine
        vb.gui = false
    
        # Customize the amount of memory on the VM:
        vb.memory = "2048"
    end
    #
    # View the documentation for the provider you are using for more
    # information on available options.
    
    # Enable provisioning with a shell script. Additional provisioners such as
    # Puppet, Chef, Ansible, Salt, and Docker are also available. Please see the
    # documentation for more information about their specific syntax and use.
    config.vm.provision "shell", inline: <<-SHELL
        apt-get -y update
        
        apt-get -y install build-essential default-jre
      
    SHELL
    
    # Print instructions after up
    config.vm.provision "shell", run: "always", inline: <<-SHELL
        echo ===============================
        echo Started the CS2200 Vagrant box!
        echo Run \\"vagrant ssh\\" to enter the box.
        echo Any files you put into the vagrant folder on the host will be accessible in the /vagrant directory in the box. Run \\"cd /vagrant\\" in the box to get to your files.
        echo Run \\"exit\\" to exit the box.
        echo Run \\"vagrant halt\\" on the host to stop the box.
        echo Run \\"vagrant up\\" on the host to restart the box.
        echo ===============================
        
    SHELL
end
