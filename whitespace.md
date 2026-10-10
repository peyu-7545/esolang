# Whitespace

```C
ss N   st.push(N);
sns    st.push(st[0]);
snt    swap(st[0], st[1]);
snn    st.pop();
sts N  st.push(st[N]);
stn N  st.remove([1, N]);

tsss   st.push(st[1] + st[0]);
tsst   st.push(st[1] - st[0]);
tssn   st.push(st[1] * st[0]);
tsts   st.push(st[1] / st[0]);
tstt   st.push(st[1] % st[0]);

tts    heap[st[1]] = st[0];
ttt    st.push(heap[st[0]]);

tnss   write((int)st[0]);
tnst   write(to_string(st[0]));
tnts   heap[st[0]] = (int)read_char();
tntt   heap[st[0]] = stoi(read_line());

nss N  label N;
nsn N  goto N;
nts N  if (st[0] == 0) goto N;
ntt N  if (st[0] < 0) goto N;
nst N  callstack.push(current); goto N;
ntn    goto callstack.pop();
nnn    exit();
```
