# GOLDILOCKS 
High performance in-memory DBMS<br>
GOLDILOCKS는 Relation DBMS의 특징과 No SQL DBMS의 확장성을 모두 갖춘 New SQL 메모리 DBMS입니다. <br>
<b>GOLDILOCKS DBMS의 기능</b>
>1.샤딩을 이용한 데이터 분산 저장 기능<br>
>2.온라인 수평 확장 및 축소 기능<br>
>3.병렬 데이터 처리 및 분산 운영 기능<br>
>4.다양한 운영 모드 지원(Active-Active, Active-StandBy 등)<br>

<b>GOLDILOCK DBMS 제품 구성</b>
>Open Software Free Product<br>
>>1. goldilocks – 고속 데이터 처리 Relation DBMS(open source)<br>
>>2. goldilocks cloud – 자동 확장 분산 데이터 처리 및 저장 가능한 Cloud DBMS(개발 중)<br>

>Commercial Software Product(Contact Email:sales@sunjesoft.com)
>>1. goldilocks cluster – 수평 확장 가능한 분산 데이터 처리 DBMS<br>
>>2. goldilocks lite – 고속 데이터 처리 Key-Value DBMS<br>


# Help
메뉴얼 다운로드 : http://www.sunjesoft.co.kr/faq <br>
채팅 및 질의응답 : http://www.sunjesoft.co.kr/faq <br>

# Q&A and Bug Reports
Q&A 또는 버그 리포트는 <http://www.sunjesoft.co.kr/faq> or support@sunjesoft.com 보내주세요.<br>
버그 패치 소스는 https://github.com/GOLDILOCKS-DBMS/GOLDILOCKS <br>

# License
godilocks는 GNU Affero General Public License version 3(GNU AGPL 3.0)에 규정에 따라 배포됩니다.<br>
>이곳에서 다운 받은 godilocks는 open software free 버전이며, godilocks cloud 버전은 개발 완료 후 오픈 소스로 등록할 예정입니다. <br>
>(오픈 소스를 위한 godilocks cloud 프로젝트가 진행 중입니다.)<br>

godilocks는 아파치 2.0 및 BSD 3-Clause “New” or “Revised” 라이센스가 포함되어 있습니다. <br>
godilocks를 수정 재배포하는 경우 아파치 2.0, BSD 라이센스 규약에 따라 재배포 되어야 합니다.<br>

라이센스는 URL 링크를 통하여 확인 할 수 있습니다.<br>
> GNU AGPL 3.0   <https://www.gnu.org/licenses/agpl-3.0><br>
> Apche 2.0      <https://www.apache.org/licenses/LICENSE-2.0><br>
> BSD 3-clause   <https://opensource.org/licenses/BSD-3-Clause><br>

# Build and Run
<b>System Information</b><br>
>OS  : Red Hat or Cent OS<br>
>CPU : x86_64<br>
>Memory : 2GB(최소) 이상<br>
>
<b>Build</b><br>
Dependent package<br>
>* gcc (4.4.7)
>* glibc (2.1)
>* ctags (5.8)
>* cmake (2.8.12.2)
>* gmake (3.81)
>* bison (2.4.1)
>* flex (2.5.35)
>* java (1.6.0)
>* doxygen (1.6.1)
>* git
>
Configuration<br>
>* System
>   * sysctl.conf (/etc/sysctl.conf)
>     * fs.file-max=65535
>     * kernel.shmall = 2097152
>     * kernel.shmmax = 4294967295
>     * kernel.shmmni = 4096
>     * kernel.sem = 250 32000 100 128
>     * net.ipv4.ip_local_port_range = 9000 65500
>     * net.core.rmem_default = 262144
>     * net.core.rmem_max = 4194304
>   * limits.conf (/etc/security/limits.conf)
>     * username soft nofile 65535
>     * username hard nofile 65535
>     * username soft nproc 65535
>     * username hard nproc 65535
>     * username soft memlock unlimited
>     * username hard memlock unlimited
>* Goldilocks
>   * export PRODUCT_HOME=/path/to/goldilocks
>   * export GOLDILOCKS_HOME=$PRODUCT_HOME/Gliese/home
>   * export GOLDILOCKS_DATA=$PRODUCT_HOME/Gliese/home
>   * export PATH=$GOLDILOCKS_HOME/bin:$PATH
>   * export LD_LIBRARY_PATH=$GOLDILOCKS_HOME/lib:$LD_LIBRARY_PATH
>
Build<br>
>* product.sh [debug | release] [parallel]
>
<b>Run</b><br>
>* recreate.sh



