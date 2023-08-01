#include "Httpconn.h"
std::string Httpconn::pre_url = "";
std::map<int, std::string> Httpconn::code_status = {
    {200, "OK"}, {400, "Bad Request"}, {403, "ForBidden"}, {404, "Not Found"}};
Httpconn::Httpconn(int sockfd_) : file(nullptr), sockfd(sockfd_), status(200)
{
}
Httpconn::~Httpconn()
{
}
void Httpconn::init()
{
    char temp[100];
    bzero(temp, 100);
    getcwd(temp, 99);
    pre_url.clear();
    pre_url = std::move(std::string(temp, temp + strlen(temp)));
}
void Httpconn::clear()
{
    if(file)munmap(file, st.st_size);
    buffer_receive.clear();
    buffer_send.clear();
    map_line.clear();
    file = nullptr;
    status = 200;
}
int Httpconn::read()
{
    clear();
    int re= buffer_receive.write_to_buffer_from_fd(sockfd);
    return re;
}
void Httpconn::process()
{
    if (!parse())
    {
        fill_send();
        return;
    }
    find_url();
    fill_send();
}
bool Httpconn::parse()
{
    int step = 0;
    while (true)
    {
        std::string t = std::move(buffer_receive.advance_until());

        switch (step)
        {
        case 0:
            parse_request(t);
            step = 1;
            break;
        case 1:
            if (t.size() == 0)
            {
                step = 2;
                break;
            }
            parse_line(t);
            break;
        case 2:
            parse_body();
            step = 3;
            break;
        default:
            break;
        }
        if (status != 200)
        {
            return false;
        }
        if (step == 3)
            return true;
    }
}
void Httpconn::parse_request(std::string &s)
{
    std::regex re("([^ ]*) ([^ ]*) [HTTPhttp/]{5}([^ ]{3})[ \t]*");
    std::smatch m;
    int sum_re = std::regex_match(s, m, re);
    if (!sum_re)
        status = 400;
    method = m[1];
    url = m[2];
    version = m[3];
    return;
}
void Httpconn::parse_line(std::string &s)
{
    std::regex re("([^ ]*) (.*)");
    std::smatch m;
    int sum_re = std::regex_match(s, m, re);
    if (!sum_re)
    {
        return;
    }
    map_line[m[1].str()] = m[2].str();
    return;
}
void Httpconn::parse_body()
{
    // 占位
}
bool Httpconn::equal_line(std::string &line, const std::string &s)
{
    int r = line.size() - 1;
    while (r >= 0 && line[r] == ' ' || line[r] == '\t')
    {
        line.pop_back();
        r--;
    }
    return line == s;
}
bool Httpconn::Keep_alive()
{
    return equal_line(map_line["Connection:"], "keep-alive");
}
void Httpconn::fill_send()
{
    std::string s;
    s += "HTTP/1.1 " + std::to_string(status) + " " + code_status[status] + "\r\n";
    if (Keep_alive() && status == 200)
        s += "Connection: keep-alive\r\n";
    else
        s += "Connection: close-live\r\n";
    if (file != nullptr)
        s += "Content-Length: " + std::to_string(st.st_size) + "\r\n";
    else
    {
        s += "Content-Length: 0\r\n";
    }
    s += "\r\n";
    buffer_send.write_to_buffer(s);
    iov[0].iov_base = buffer_send.ptr_start();
    iov[0].iov_len = buffer_send.size();
    iov[1].iov_base = file;
    if (file)
        iov[1].iov_len = st.st_size;
    else
        iov[1].iov_len = 0;
}
bool Httpconn::find_url()
{
    if (url == "/")
    {
        url = "/3.html";
    }

        std::string all_url=(pre_url + "/html" + url);
    int fd = open(all_url.data(), O_RDONLY);
    if (fd <= 0)
    {
        int re = errno;
        if (re & EACCES)
        {
            status = 404;
        }
        else if (re & (ENOENT | EISDIR))
        {
            status = 403;
        }
        else
        {
            status = 403;
        }
    }
    if (status != 200)
    {
        close(fd);
        return false;
    }
   //Log::Instance()->write(std::to_string(sockfd)+" request "+all_url+"  status: "+std::to_string(status));
    stat(all_url.data(), &st);
    char *temp = (char *)mmap(NULL, (st.st_size), PROT_READ, MAP_PRIVATE, fd, 0);
    if (temp != MAP_FAILED)
    {

        file = (char *)temp;
    }
    else
    {
        status = 403;
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
bool Httpconn::send()
{
    if(iov[0].iov_len + iov[1].iov_len<=0){
      clear();  return true;}
    int len=writev(sockfd,iov,2);
    if(len<0)return true;
    if(len>iov[0].iov_len)
    {
        len-=iov[0].iov_len;
        iov[0].iov_len=0;
        iov[1].iov_len-=len;
        iov[1].iov_base+=len;
    } 
    else
    {
        iov[0].iov_len-=len;
        iov[0].iov_base+=len;
    }
    if(iov[0].iov_len+iov[1].iov_len<=0){ clear();  ;return true;}
    return false;
}