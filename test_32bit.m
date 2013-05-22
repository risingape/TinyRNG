
disp('Uniform, 32-bit XORSHIFT');
fid = fopen('testdata/unif01_xorshift32.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

disp('Uniform, 32-bit KISS');
fid = fopen('testdata/unif01_kiss32.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, unifrnd(0, 1, length(data), 1))

disp('Transformations using the KISS generator');
disp('Exponential');
fid = fopen('testdata/exp1_32.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, exprnd(1, length(data), 1))

disp('Standard normal')
fid = fopen('testdata/normstd_32.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, normrnd(0, 1, length(data), 1))

disp('Normal');
fid = fopen('testdata/norm55_32.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, normrnd(5, 5, length(data), 1))

disp('Poisson');
fid = fopen('testdata/poiss5_32.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, poissrnd(5,  length(data), 1))

disp('Gamma');
fid = fopen('testdata/gamm51_32.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);
[h, p, k] = kstest2(data, gamrnd(9.0, 0.5,  length(data), 1))

disp('Beta')
fid = fopen('testdata/beta25_32.txt', 'r');
data = fscanf(fid, '%f');
fclose(fid);

[h, p, k] = kstest2(data, betarnd(2, 5,  length(data), 1))
