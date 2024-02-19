#pragma once

class ImageFile {
public:
    ImageFile() = default;
    ~ImageFile() { Destroy(); }

public:
    void Init(const std::string& path);
    void Destroy(void);

public:
    int GetWidth(void) const { return m_width; }
    int GetHeight(void) const { return m_height; }
    int GetSize(void) const { return m_width * m_height * 4; }
    uint8_t* GetData(void) const { return p_data; }

private:
    int m_width {};
    int m_height {};
    int m_channels {};
    uint8_t* p_data {};
};
