## behat setup
* download (and unzip) selenium (3.13 works) to both your VM and to your local (/opt is a fine place, /vagrant (on the VM) might be even better).
http://selenium-release.storage.googleapis.com/index.html?path=3.13/
* composer install behat either globally or in a module's /tests directory
* Start selenium on both the VM and on your local
  * [local] in a bare (no screen, tmux, byobu) terminal, run `java -jar /opt/selenium-server-standalone-3.13.0.jar -role node -hub http://192.168.111.111:4444/wd/hub`
  * [vm] `java -jar /vagrant/selenium-server-standalone-3.13.0.jar -role hub`
* finally, run behat for your project, for example
  * `cd /var/www/ldl/sites/all/modules/islandora_content_stats/tests && bin/behat -v`
