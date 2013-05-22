
disp('Uniform, 64-bit XORSHIFT');
fid = fopen('testdata/unif01_xorshift64.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, unifrnd(0, 1, length(data), 1))

disp('Uniform, 64-bit KISS');
fid = fopen('testdata/unif01_kiss64.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, unifrnd(0, 1, length(data), 1))

disp('Transformations using the KISS generator');
disp('Exponential');
fid = fopen('testdata/exp1_64.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, exprnd(1, length(data), 1))

disp('Standard normal');
fid = fopen('testdata/normstd_64.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, normrnd(0, 1, length(data), 1))

disp('Normal');
fid = fopen('testdata/norm55_64.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, normrnd(5, 5, length(data), 1))

disp('binomial');
fid = fopen('testdata/bino2005_64.txt', 'r');
data = fscanf(fid, '%d');
fclose(fid);

[h, p, k] = kstest2(data, binornd(20, 0.5, length(data), 1))

disp('Poisson');
fid = fopen('testdata/poiss5_64.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, poissrnd(5,  length(data), 1))

disp('Gamma');
fid = fopen('testdata/gamm51_64.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);
hist(data, 500);

[h, p, k] = kstest2(data, gamrnd(5.0, 1.0,  length(data), 1))

disp('Beta');
fid = fopen('testdata/beta25_64.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, betarnd(2, 5,  length(data), 1))
