run project oneliner
cd /var/uc3m/p3 && chmod +x corrector_ssoo_p3.sh && make clean && export LD_LIBRARY_PATH=/var/uc3m/p3:$LD_LIBRARY_PATH && make && ./calculator test 3 3


CORRECT project oneliner:

cd /var/uc3m/p3 && rm -rf testdir && rm -rf ssoo_p3_100405803_100428965.zip && zip -r ssoo_p3_100405803_100428965.zip ./* && chmod +x corrector_ssoo_p3.sh && make clean && export LD_LIBRARY_PATH=/var/uc3m/p3:$LD_LIBRARY_PATH && make && ./corrector_ssoo_p3.sh ssoo_p3_100405803_100428965.zip