<h1>LSB method for a BMP file</h1>
<h2>A steganography program to hide a message with LSB-method in a BMP file

</h2>
<img src=''>

<br>
<hr>
<h2>About it</h2>

<p>This code provides a tool to create a BMP image from a given pattern, write a message to a BMP and read it.</p>

<br>
<hr>
<h2>How to use it</h2>

<p>Compile the code using:</p>
<code>gcc patterns.c bmp.c main.c -o main</code>

<b>Note that on linux it needs the math library to be linked, so add the <code>-lm</code> flag.</b>

<p>The program has a builtin CLI that has 3 options, to create, write and read a BMP.</p>

<p>To encode a message to a BMP, the text should be written in a file and the BMP should also exists.</p>

<p>In the <a href='https://github.com/w-i-l/lsb-method-bmp/blob/main/patterns.c'>patterns.c</a> file, are hardcoded 3 different styles for creating a BMP. To add a new file simply define the function, add it to the <a href='https://github.com/w-i-l/lsb-method-bmp/blob/main/Headers/patterns.h'>header file</a> and then write it in the patterns functions array in <a href='https://github.com/w-i-l/lsb-method-bmp/blob/main/Headers/patterns.h'>main.c line 17</a> .</p>

<br>
<hr>
<h2>How it works</h2>

<p>The following code takes at a time a BGR(blue-green-red) struct and alter the last bit based on the current bit in the message the writes the altered stuct in a new file.</p>

<br>
<hr>
<h2>Tech specs</h2>

<p>The program is built to mainly work with the following BMP format:</p>

<table>
    <tr>
        <th>Number of colors planes</th>
        <th>Number of bits per pixel</th>
        <th>Compression</th>
        <th>Resolution</th>
        <th>Number of colors used</th>
        <th>Number of important colors</th>
    </tr>
    <tr>
        <td>1 plane</td>
        <td>24 bits</td>
        <td>BI_RGB (none)</td>
        <td>2835 pixels/metre horizontal</td>
        <td>0 colors</td>
        <td>0 colors</td>
    </tr>
</table>

<b>Note that this implementation won't work with RGBA.</b>

<hr>

<p>The writting part can be a little bit confusing, so below it is an explanation:</p>

<code> bit = ((message[index] & (1 << (no_of_bits - 1) )) >> (no_of_bits - 1));</code>

<p>Taking the inner parenthesis <code>1 << (no_of_bits - 1)</code> simply creates a mask to get the bit from the possition <code>no_of_bits -1 </code>. Then we <b>logical and</b> with the message to get the bit from the given position, but we also could received another bits so we <b>rigth shift</b> with the same amount that we have shifted to left to only get the bit that we were looking for.</p>
<br/><br/>

<code>aux.b = ((aux.b >> 1) << 1) | bit;</code>

<p>Here we overwrite the bit to the current BGR stuct. The inner paranthesis simply "deletes" the last bit, then we <b>left shift by one</b> to "create" a 0 bit and finally write our bit with a <b>logical or</b>.</p>

<p>To visualise all the process supose that we want to write the letter <b>e</b>(<code>                 0110 0101</code>).</p>

<p>We start with the index 0 in the current block(meaning that we didn't write any bit) so <code>no_of_bits = 1</code> because we count from 1.</p>

<p>Shifting <code>1 << (1-1 = 0) = 0</code> gives 0 so we basically take the last bit from <b>e</b> which is <code>1</code>.</p>

<p>Now suppose that our <code>aux.b</code> which is a char and represents the amount of blue in the curent pixel is a random value, let's say <code>0110 0111</code>.</p>

<p>When we <b>right shift by one</b> we get <code>0110 011</code> (this representation will help with visualising the process). After <b>left shift by one</b> the char become <code>0110 0110</code>. So now when we <b>logical or</b> we gonna overwrite the last bit and get the final result which is <code>0110 0111</code>. We do the same for the rest of bits.</p>

<p>The reading part is a lot easier, we just simply <b>left shift by one</b> to "create" and empty bit and the we write our bit read from the BMP.</p>

<code>buffer[buffer_index] <<= 1;<br>
                buffer[buffer_index] |= aux.b & 1;</code>

