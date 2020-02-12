#include <bzlib.h>
#include <streambuf>
#include <istream>
#include <vector>
#include <cstring>     // memcpy


class BZ2IStreamBuf : public std::streambuf
{
public:
    BZ2IStreamBuf(size_t bs = 8192) : buf(bs) {
        setp(buf.data(), buf.data() + (buf.size() - 1));
        setg(buf.data() + 4, buf.data() + 4, buf.data() + 4);
    }

    BZ2IStreamBuf& open(const char *fname) {
        bzfile = BZ2_bzopen(fname, "rb");
        if (!bzfile)
            throw std::runtime_error(std::string("bz_open file \"") + fname + "\"");
        return *this;
    }

    virtual std::streambuf::int_type underflow() override {
        if ((gptr() != nullptr) && (gptr() < egptr()))
            return *reinterpret_cast<unsigned char*>(gptr());

        if (!bzfile)
            return EOF;

        // Josuttis' implementation of inbuf
        auto n_putback = 0l;
        if (gptr() != nullptr) {
            n_putback = std::min(gptr() - eback(), 4l);
            memcpy(buf.data() + (4 - n_putback), gptr() - n_putback,
                   static_cast<std::size_t>(n_putback));
        }

        int bzerror;
        int num = BZ2_bzRead(&bzerror, bzfile, buf.data() + 4, buf.size() - 4);
        if (!(bzerror == BZ_OK || (bzerror == BZ_STREAM_END && num > 0)))
            return EOF;

        // reset buffer pointers
        setg(buf.data() + (4 - n_putback),  // beginning of putback area
             buf.data() + 4,                // read position
             buf.data() + 4 + num);         // end of buffer

        // return next character
        return *reinterpret_cast<unsigned char*>(gptr());
    }

    void close() {
        if (bzfile) BZ2_bzclose(bzfile);
        int errnum;
        auto msg = BZ2_bzerror(bzfile, &errnum);
        if (errnum != BZ_OK)
            throw std::runtime_error(std::string("bzclose: ") + msg);
    }

    ~BZ2IStreamBuf() { close(); }

private:
    BZFILE *bzfile = nullptr;
    std::vector<char> buf = {};
};


class BZ2IStream : public std::istream
{
public:
    BZ2IStream(const char *fname, size_t bs = 8192) : std::istream(&buf_), buf_(bs)
        { buf_.open(fname); }

private:
    BZ2IStreamBuf buf_;
};
