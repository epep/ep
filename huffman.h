#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <string>
#include <queue>
#include <vector>
#include <fstream>
using namespace std;

/**
 * @brief 哈夫曼树节点
 *
 */
struct huffman_node
{
    /* 字符 */
    char id;

    /* 字符的频度 */
    int freq;

    /* 字符的哈夫曼编码 */
    string code;

    /* 哈夫曼树左节点 */
    huffman_node *left;

    /* 哈夫曼树右节点 */
    huffman_node *right;

    /* 哈夫曼树构造函数 */
    huffman_node()
    {
        left = right = NULL;
    }
};
typedef huffman_node *PNode;

/**
 * @brief 哈夫曼类
 *
 */


class Huffman {
protected:

    /* 用于存放 256 种类型的字符 */
    PNode node_array[256];

    /* 输入和输出文件流 */
    fstream in_file, out_file;

    /* 哈夫曼树的根节点,子节点,和父节点 */
    PNode child, parent, root;
    char id;

    /* 输入和输出文件名 */
    string in_file_name, out_file_name;

    /**
     * @brief 一个仿函数,用于比较树节点中的频度的大小,在优先队列中使用
     */
    class compare {
public:

        bool operator()(const PNode& c1, const PNode& c2) const
        {
            return c1->freq > c2->freq;
        }
    };

    /* 优先队列,在生成哈夫曼树时会使用,其他的方法可以使用最小堆 */
    priority_queue<PNode, vector<PNode>, compare>pq;
    void        create_node_array();
    void        traverse(PNode,
                         string);
    int         binary_to_decimal(const string&);
    string      decimal_to_binary(int);
    inline void build_tree(string&,
                           char);

public:

    Huffman(string,
            string);
    void create_pq();
    void create_huffman_tree();
    void calculate_huffman_codes();
    void coding_save();
    void decoding_save();
    void recreate_huffman_tree();
};

#endif // ifndef HUFFMAN_H
