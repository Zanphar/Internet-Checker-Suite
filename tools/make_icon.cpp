// Internet Connection Utility - icon generator
// Copyright (c) 2026 Charles McDonald. All rights reserved.
// SPDX-License-Identifier: LicenseRef-Proprietary
//
// Renders res/icon.ico (16-256 px, BMP frames up to 64 px, PNG above) plus a
// magnified preview sheet used to check legibility at small sizes.

#include <windows.h>
#include <gdiplus.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace Gdiplus;

namespace {

void RoundPath(GraphicsPath& p, RectF r, float rad) {
    float d = rad * 2;
    p.AddArc(r.X, r.Y, d, d, 180, 90);
    p.AddArc(r.X + r.Width - d, r.Y, d, d, 270, 90);
    p.AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0, 90);
    p.AddArc(r.X, r.Y + r.Height - d, d, d, 90, 90);
    p.CloseFigure();
}

void Arc(Graphics& g, const Pen* pen, float x, float y, float w, float h, float a, float s) {
    g.DrawArc(pen, (REAL)x, (REAL)y, (REAL)w, (REAL)h, (REAL)a, (REAL)s);
}

// The mark: gradient tile, signal arcs radiating from the lower centre, ICU monogram.
void DrawMark(Graphics& g, float S) {
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);

    RectF tile(S * 0.035f, S * 0.035f, S * 0.93f, S * 0.93f);
    GraphicsPath p;
    RoundPath(p, tile, S * 0.235f);

    LinearGradientBrush bg(RectF(0, 0, S, S), Color(255, 0x23, 0xC9, 0xB7),
                           Color(255, 0x3F, 0x6F, 0xEA), LinearGradientModeForwardDiagonal);
    g.FillPath(&bg, &p);
    Pen edge(Color(46, 2, 16, 26), std::max(1.f, S * 0.012f));
    g.DrawPath(&edge, &p);

    // Soft top highlight for depth.
    GraphicsPath* clip = p.Clone();
    g.SetClip(clip);
    LinearGradientBrush gloss(RectF(tile.X, tile.Y, tile.Width, tile.Height * 0.55f),
                              Color(46, 255, 255, 255), Color(0, 255, 255, 255),
                              LinearGradientModeVertical);
    g.FillRectangle(&gloss, RectF(tile.X, tile.Y, tile.Width, tile.Height * 0.55f));

    // Connection signal: concentric arcs opening upward from the lower centre.
    // Below ~28 px the arcs turn to noise, so tiny frames stay a clean monogram.
    const float cx = S * 0.5f, cy = S * 0.94f;
    if (S >= 28.f) {
        const float radii[3] = {0.28f, 0.40f, 0.52f};
        const int alphas[3] = {125, 88, 55};
        for (int i = 0; i < 3; ++i) {
            float r = radii[i] * S;
            Pen pen(Color(alphas[i], 3, 26, 40), std::max(1.2f, S * 0.045f));
            pen.SetStartCap(LineCapRound);
            pen.SetEndCap(LineCapRound);
            Arc(g, &pen, cx - r, cy - r, r * 2, r * 2, 202, 136);
        }
        SolidBrush origin(Color(150, 3, 26, 40));
        g.FillEllipse(&origin, cx - S * 0.035f, cy - S * 0.035f, S * 0.07f, S * 0.07f);
    }

    // Monogram.
    FontFamily fam(L"Segoe UI");
    // Keep the monogram inside the tile at every size; tiny frames get a smaller
    // scale so the three letters never touch the edges.
    float fscale = (S <= 20.f) ? 0.34f : (S <= 40.f) ? 0.38f : 0.42f;
    Font font(&fam, S * fscale, FontStyleBold, UnitPixel);
    StringFormat fmt;
    fmt.SetAlignment(StringAlignmentCenter);
    fmt.SetLineAlignment(StringAlignmentCenter);
    fmt.SetFormatFlags(StringFormatFlagsNoWrap);
    fmt.SetTrimming(StringTrimmingNone);

    RectF box(0, S * 0.13f, S, S * 0.62f);
    SolidBrush shadow(Color(95, 3, 24, 38));
    g.DrawString(L"ICU", -1, &font, RectF(box.X, box.Y + S * 0.018f, box.Width, box.Height), &fmt,
                 &shadow);
    SolidBrush white(Color(255, 255, 255, 255));
    g.DrawString(L"ICU", -1, &font, box, &fmt, &white);

    g.ResetClip();
    delete clip;
}

Bitmap* RenderFrame(int size) {
    auto* bmp = new Bitmap(size, size, PixelFormat32bppARGB);
    Graphics g(bmp);
    g.Clear(Color(0, 0, 0, 0));
    DrawMark(g, (float)size);
    return bmp;
}

struct Frame {
    int size;
    std::vector<unsigned char> bytes;
    bool png;
};

void Put32(std::vector<unsigned char>& v, unsigned int x) {
    v.push_back((unsigned char)(x & 0xFF));
    v.push_back((unsigned char)((x >> 8) & 0xFF));
    v.push_back((unsigned char)((x >> 16) & 0xFF));
    v.push_back((unsigned char)((x >> 24) & 0xFF));
}

void Put16(std::vector<unsigned char>& v, unsigned short x) {
    v.push_back((unsigned char)(x & 0xFF));
    v.push_back((unsigned char)((x >> 8) & 0xFF));
}

std::vector<unsigned char> BitmapToDib(Bitmap* bmp, int size) {
    std::vector<unsigned char> out;
    BitmapData data{};
    Rect rc(0, 0, size, size);
    if (bmp->LockBits(&rc, ImageLockModeRead, PixelFormat32bppARGB, &data) != Ok) return out;

    int maskStride = ((size + 31) / 32) * 4;
    int maskSize = maskStride * size;
    int pixelSize = size * size * 4;

    Put32(out, 40);                     // biSize
    Put32(out, (unsigned)size);         // biWidth
    Put32(out, (unsigned)(size * 2));   // biHeight (XOR + AND)
    Put16(out, 1);                      // planes
    Put16(out, 32);                     // bit count
    Put32(out, 0);                      // BI_RGB
    Put32(out, (unsigned)(pixelSize + maskSize));
    Put32(out, 0);
    Put32(out, 0);
    Put32(out, 0);
    Put32(out, 0);

    auto* base = (unsigned char*)data.Scan0;
    for (int y = size - 1; y >= 0; --y) {
        out.insert(out.end(), base + y * data.Stride, base + y * data.Stride + size * 4);
    }
    out.insert(out.end(), (size_t)maskSize, 0);
    bmp->UnlockBits(&data);
    return out;
}

int EncoderClsid(const WCHAR* mime, CLSID* clsid) {
    UINT num = 0, bytes = 0;
    GetImageEncodersSize(&num, &bytes);
    if (bytes == 0) return -1;
    std::vector<unsigned char> buf(bytes);
    auto* info = (ImageCodecInfo*)buf.data();
    GetImageEncoders(num, bytes, info);
    for (UINT i = 0; i < num; ++i) {
        if (wcscmp(info[i].MimeType, mime) == 0) {
            *clsid = info[i].Clsid;
            return (int)i;
        }
    }
    return -1;
}

bool EncodePng(Bitmap* bmp, std::vector<unsigned char>& out) {
    CLSID clsid{};
    if (EncoderClsid(L"image/png", &clsid) < 0) return false;
    IStream* stream = nullptr;
    if (CreateStreamOnHGlobal(nullptr, TRUE, &stream) != S_OK) return false;
    if (bmp->Save(stream, &clsid, nullptr) != Ok) {
        stream->Release();
        return false;
    }
    STATSTG stat{};
    stream->Stat(&stat, STATFLAG_NONAME);
    out.resize((size_t)stat.cbSize.QuadPart);
    LARGE_INTEGER zero{};
    stream->Seek(zero, STREAM_SEEK_SET, nullptr);
    ULONG read = 0;
    stream->Read(out.data(), (ULONG)out.size(), &read);
    stream->Release();
    return read == out.size();
}

bool SaveIco(const wchar_t* path, const std::vector<int>& sizes) {
    std::vector<Frame> frames;
    for (int size : sizes) {
        Bitmap* bmp = RenderFrame(size);
        Frame f;
        f.size = size;
        f.png = size >= 128;
        if (f.png) {
            if (!EncodePng(bmp, f.bytes)) {
                delete bmp;
                return false;
            }
        } else {
            f.bytes = BitmapToDib(bmp, size);
        }
        delete bmp;
        frames.push_back(std::move(f));
    }

    std::vector<unsigned char> header;
    Put16(header, 0);
    Put16(header, 1);
    Put16(header, (unsigned short)frames.size());

    unsigned int offset = 6 + 16 * (unsigned int)frames.size();
    for (auto& f : frames) {
        header.push_back((unsigned char)(f.size >= 256 ? 0 : f.size));
        header.push_back((unsigned char)(f.size >= 256 ? 0 : f.size));
        header.push_back(0);
        header.push_back(0);
        Put16(header, 1);
        Put16(header, 32);
        Put32(header, (unsigned)f.bytes.size());
        Put32(header, offset);
        offset += (unsigned)f.bytes.size();
    }
    for (auto& f : frames) header.insert(header.end(), f.bytes.begin(), f.bytes.end());

    FILE* fp = nullptr;
    if (_wfopen_s(&fp, path, L"wb") != 0 || !fp) return false;
    fwrite(header.data(), 1, header.size(), fp);
    fclose(fp);
    return true;
}

// Preview sheet: 256 px plus small sizes magnified with nearest-neighbour.
void SavePreview(const wchar_t* path) {
    const int pad = 16;
    int w = pad + 256 + pad + 192 + pad + 192 + pad + 192 + pad;
    int h = 320;
    Bitmap sheet(w, h, PixelFormat32bppARGB);
    Graphics g(&sheet);
    g.Clear(Color(255, 12, 18, 28));
    g.SetInterpolationMode(InterpolationModeNearestNeighbor);

    Bitmap* big = RenderFrame(256);
    g.DrawImage(big, pad, 22, 256, 256);
    delete big;

    int x = pad + 256 + pad;
    struct {
        int size, zoom;
    } smalls[3] = {{64, 3}, {32, 6}, {16, 12}};
    for (auto& s : smalls) {
        Bitmap* b = RenderFrame(s.size);
        g.DrawImage(b, x, 22, s.size * s.zoom, s.size * s.zoom);
        delete b;
        x += s.size * s.zoom + pad;
    }

    CLSID clsid{};
    if (EncoderClsid(L"image/png", &clsid) >= 0) sheet.Save(path, &clsid, nullptr);
}

} // namespace

int main() {
    GdiplusStartupInput gi;
    ULONG_PTR token = 0;
    if (GdiplusStartup(&token, &gi, nullptr) != Ok) return 1;

    CreateDirectoryW(L"res", nullptr);
    bool ok = SaveIco(L"res\\icon.ico", {16, 20, 24, 32, 40, 48, 64, 128, 256});
    SavePreview(L"res\\icon_preview.png");

    GdiplusShutdown(token);
    if (!ok) {
        printf("icon generation failed\n");
        return 1;
    }
    printf("wrote res\\icon.ico and res\\icon_preview.png\n");
    return 0;
}
