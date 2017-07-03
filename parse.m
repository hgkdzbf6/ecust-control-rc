data=csvread('data30hz.csv');
subplot(221);
stem(data(3:end));
title('30hz');

data2=csvread('data20hz.csv');
subplot(222);
stem(data2(3:end));
title('20hz');

data3=csvread('data10hz.csv');
subplot(223);
stem(data3(3:end));
title('10hz');

data4=csvread('data50hz.csv');
subplot(224);
stem(data4(3:end));
title('50hz');