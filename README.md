### Average Calculator (AvgCalculator.c)
- MPI_Send ve MPI_Recv fonksiyonları kullanılmaktadır.
- 20 elemanlı int array'in 5 birimlik chunk'lara bölünerek 4 slave makine tarafından distributed olarak hesaplanmaktaıdr.

### Ceaser Cipher (CeaserCipher.c)
- MPI_Scatter ve MPI_Gather fonksiyonları kullanılmaktadır.
- Girdi olarak alınan metin Scatter komutu ile  master node’tan tüm slave node’lara dağıtılmaktadır.
- Scatter ile alınan veri, Ceasar cipher algoritması kullanılarak encrypt edilmektedir.
- Encrypt edilmiş veriler, Gather komutu kullanılarak master node’ta toplanılmaktadır.

### Standard Sapma Hesaplama Uygulaması (StandardDeviation.c)
- MPI_Reduce ve MPI_Allreduce fonksiyonları kullanılmaktadır.
- Slave node’larda X adet random sayı üretilmekte ve local olarak toplanmaktadır.
- Allreduce ile tüm slave node’lardaki local toplamlar toplanarak, global toplam elde edilmektedir.
- Tüm slave node’larda, global toplam üzerinden ortalama hesaplanmaktadır.
- Tüm slave node’larda, random sayılar ve ortalama kullanılarak kare farkları hesaplanmaktadır.
- Reduce ile local de toplanılan kare farkları master node’a toplanmaktadır.
- Master node’ta **sqrt(global kare farkları toplamı / X * size)** kullanılarak standard sapma hesaplanmaktadır.

### Nümerik İntegral Hesaplama Uygulaması (NumericalIntegral.c)
- MPI_Send, MPI_Receive, MPI_Pack, MPI_Unpack, MPI_Scatter ve MPI_Reduce fonksiyonları kullanılmaktadır.
- Uygulamada Simpson yöntemi kullanılarak nümerik yöntemlerle integral hesaplanmaktadır.
- Uygulamada hesaplanacak fonksiyon **f(x)=cos(x)** olarak belirlenmiştir.
- Master node n(adım sayısı), a ve b(belirli integral aralığı) değerlerini Pack komutu ile paketleyip Send komutu ile slave node’lara göndermektedir.
- Slave node’lar ise Receive komutu ile paketi almakta ve Unpack komutu ile paketten n, a  ve b değerlerini elde etmektedir.
- **h(Δx)** değeri tüm slave node’larda hesaplanmaktadır.
- Master node’ta tüm local a değerleri hesaplanmakta ve Scatter komutu ile tüm node’lara dağıtılmaktadır.
- Tüm slave node’larda local b değerleri hesaplanmaktadır.
- Tüm slave node’larda, Simpson methodu ile hesaplama yapılmaktadır.
- Reduce komutuyla, slave node'larda hesaplanan tüm değerler Master node’ta toplanmaktadır.
