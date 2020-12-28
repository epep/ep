#include "huffman.h"

/**
 * @brief 初始化节点数组,给每一个元素的 id,赋值为 0-255,共256中可能
 *
 */
void Huffman::create_node_array()
{
    for (int i = 0; i < 256; i++)
    {
        node_array[i] = new huffman_node;
        node_array[i]->id = i;
        node_array[i]->freq = 0;
    }
}

/**
 * @brief 遍历哈夫曼树,用于生成哈夫曼编码
 *
 * @param node 节点
 * @param code 生成的哈夫曼编码
 */
void Huffman::traverse(PNode node, string code)
{
    if ((node->left == NULL) && (node->right == NULL))
    {
        node->code = code;
    }
    else
    {
        traverse(node->left,  code + '0');
        traverse(node->right, code + '1');
    }
}

/**
 * @brief 辅助函数,用于将编码后的文件存入文件时,将二进制转为十进制
 *                用于减小文件大小
 * @param in  8 位二进制字符串
 * @return int 转换后的十进制数
 */
int Huffman::binary_to_decimal(const string& in)
{
    int result = 0;

    for (size_t i = 0; i < in.size(); i++) result = result * 2 + in[i] - '0';
    return result;
}

/**
 * @brief 辅助函数,用于在解码文件时,将十进制的数,转换为二进制数
 *
 * @param in 待转换的十进制数
 * @return string 转换后的字符串
 */
string Huffman::decimal_to_binary(int in)
{
    string temp = "";
    string result = "";

    while (in)
    {
        temp += ('0' + in % 2);
        in /= 2;
    }

    /* 如果字符串不足 8 位,用前导 0 补齐 */
    result.append(8 - temp.size(), '0');

    for (int i = temp.size() - 1; i >= 0; i--)
    {
        result += temp[i];
    }
    return result;
}

/**
 * @brief 构造哈夫曼树
 *
 * @param path
 * @param a_code
 */
inline void Huffman::build_tree(string& path, char a_code)
{
    PNode current = root;

    for (size_t i = 0; i < path.size(); i++)
    {
        if (path[i] == '0')
        {
            if (current->left == NULL) current->left = new huffman_node;
            current = current->left;
        }
        else if (path[i] == '1')
        {
            if (current->right == NULL) current->right = new huffman_node;
            current = current->right;
        }
    }
    current->id = a_code; // attach id to the leaf
}

/**
 * @brief 哈夫曼类的构造函数
 *
 * @param in 输入的文件名称
 * @param out 输出的文件名称
 */
Huffman::Huffman(string in, string out)
{
    in_file_name = in;
    out_file_name = out;
    create_node_array();
}

/**
 * @brief 创建优先队列,
 * @note 这里在文件打开的时候,愚蠢三 binary 标志,否则
 *       会以文本方式打开,会出现文本无法读取的现象,
 */
void Huffman::create_pq()
{
    in_file.open(in_file_name, ios::in | ios::binary);
    in_file.get(id);

    /* 统计文件中字符的频度 */
    while (!in_file.eof())
    {
        node_array[(unsigned char)id]->freq++;
        in_file.get(id);
    }
    in_file.close();

    /* 将字符的频度导入到优先队列中,用于后续的哈夫曼树的生成 */
    for (int i = 0; i < 256; i++)
    {
        if (node_array[i]->freq)
        {
            pq.push(node_array[i]);
        }
    }
}

/**
 * @brief 构造哈夫曼树,
 * @note 这里的构造过程和我们手动构造的过程时一致的,
 *               取最小的两个节点,合并成一个节点,并加入到队列中
 */
void Huffman::create_huffman_tree()
{
    priority_queue<PNode, vector<PNode>, compare> temp(pq);

    while (temp.size() > 1)
    {
        root = new huffman_node;
        root->freq = 0;

        /* 取出两个频度最小的节点,合并成新的节点,加入到队列中,直到没有节点 */
        root->left = temp.top();
        root->freq += temp.top()->freq;
        temp.pop();
        root->right = temp.top();
        root->freq += temp.top()->freq;
        temp.pop();
        temp.push(root);
    }
}

/**
 * @brief 计算哈夫曼编码,并存放到各自的 code 字段中
 *
 */
void Huffman::calculate_huffman_codes()
{
    traverse(root, "");
}

/**
 * @brief 将文件编码后保存
 * @note 这里同意需要注意的是,文件打开和保存时要使用 binary 选项,
 *               否则在编码一些非文本的文件时,出现错误
 *
 * @note 文件格式说明:第1 2 个字节用于存储优先队列的大小,然后一次是字符及其对应的哈夫曼编码
 *       哈夫曼编码使用的是 256 位的长度,然后就是文件编码后的内容
 */
void Huffman::coding_save()
{
    /* 打开文件 */
    in_file.open(in_file_name, ios::in | ios::binary);
    out_file.open(out_file_name, ios::out | ios::binary);
    string in = "", s = "";

    /* 存入优先队列的大小,即字符的种类 */
    in += (char)((pq.size() & 0xFF00) >> 8);
    in += (char)(pq.size() & 0xFF);
    priority_queue<PNode, vector<PNode>, compare> temp(pq);

    /* 获取字符的哈夫曼编码,并将其存入到文件中 */
    while (!temp.empty())
    {
        PNode current = temp.top();
        in += current->id;
        s.assign(255 - current->code.size(), '0');

        /* 这里的字符 1 用于表示哈夫曼编码的开始 */
        s += '1';
        s.append(current->code);
        in += (char)binary_to_decimal(s.substr(0, 8));

        /* 将其转换为十进制存入到文件中 */
        for (int i = 0; i < 31; i++)
        {
            s = s.substr(8);
            in += (char)binary_to_decimal(s.substr(0, 8));
        }
        temp.pop();
    }
    s.clear();

    in_file.get(id);

    while (!in_file.eof())
    {
        /* 获取字符对应的哈夫曼编码 */
        s += node_array[(unsigned char)id]->code;

        /* 如果此时的哈夫曼编码的长度大于 8 的话,将其转换为十进制,存入到文件中 */
        while (s.size() > 8)
        {
            in += (char)binary_to_decimal(s.substr(0, 8));
            s = s.substr(8);
        }
        in_file.get(id);
    }
    int count = 8 - s.size();

    /* 记录文件最后一个可能不足 8 位的哈夫曼编码的长度,并将其补齐存入到文件中 */
    if (s.size() < 8)
    {
        s.append(count, '0');
    }
    in += (char)binary_to_decimal(s);
    in += (char)count;

    /* 写入文件,并关闭文件 */
    out_file.write(in.c_str(), in.size());
    in_file.close();
    out_file.close();
}

/**
 * @brief 根据编码后文件头部的哈夫曼编码重新构造哈夫曼树
 * @note  这里文件打开时,也要使用 binary 选项
 */
void Huffman::recreate_huffman_tree()
{
    char buf[10] = { 0 };

    in_file.open(in_file_name, ios::in | ios::binary);
    int size;

    /* 读取文件大小 */
    in_file.read(buf, 2);
    size = buf[0] * 256 + buf[1];

    root = new huffman_node;

    /* 构造哈夫曼树 */
    for (int i = 0; i < size; i++)
    {
        char a_code;
        unsigned char h_code_c[32];
        in_file.read(&a_code,                            1);
        in_file.read(reinterpret_cast<char *>(h_code_c), 32);
        string h_code_s = "";

        for (int i = 0; i < 32; i++)
        {
            h_code_s += decimal_to_binary(h_code_c[i]);
        }
        int j = 0;

        while (h_code_s[j] == '0')
        {
            j++;
        }
        h_code_s = h_code_s.substr(j + 1);
        build_tree(h_code_s, a_code);
    }
    in_file.close();
}

/**
 * @brief 解码文件
 * @note 文件打开时,需要使用 binary 选项
 *
 */
void Huffman::decoding_save()
{
    char buf[10] = { 0 };

    in_file.open(in_file_name, ios::in | ios::binary);
    out_file.open(out_file_name, ios::out | ios::binary);

    /* 哈夫曼树的大小 */
    int size;

    in_file.read(buf, 2);
    size = buf[0] * 256 + buf[1];

    /* 跳到文件最后,获取最后一个不足 8 位的哈夫曼编码的长度 */
    in_file.seekg(-1, ios::end);
    char count0;

    in_file.read(&count0, 1);

    /* 跳到文件实际内容开始的地方,开始解码 */
    in_file.seekg(2 + 33 * size, ios::beg); // jump to the position where text starts

    vector<unsigned char> text;
    unsigned char textseg;

    in_file.read(reinterpret_cast<char *>(&textseg), 1);

    /* 将编码后的文件全部读取进来 */
    while (!in_file.eof())
    {
        text.push_back(textseg);
        in_file.read(reinterpret_cast<char *>(&textseg), 1);
    }
    PNode  current = root;
    string path;

    // 根据哈夫曼编码找到对应的字符
    for (size_t i = 0; i < text.size() - 1; i++)
    {
        path = decimal_to_binary(text[i]);

        if (i == text.size() - 2) path = path.substr(0, 8 - count0);

        for (size_t j = 0; j < path.size(); j++)
        {
            if (path[j] == '0') current = current->left;
            else current = current->right;

            if ((current->left == NULL) && (current->right == NULL))
            {
                out_file.put(current->id);
                current = root;
            }
        }
    }

    /* 关闭文件 */
    in_file.close();
    out_file.close();
}
