//
// Created by Maddie on 9/14/2024.
//

#ifndef PROJECT_H
#define PROJECT_H

#include <Editor/MainMenuBar.h>
#include <FileSystem/FileManager.h>

namespace OpenShaderDesigner
{
    
class Project : public MainMenuBar, public Asset
{
public:
    Project();
    virtual ~Project();

    void DrawMenuBar() override;

    void Open() override;
    void Load(const FileManager::Path& path) override;
    void Save(const FileManager::Path& path) override;
    void Create(const FileManager::Path& path);

private:
    void Reset();
    
    FileManager::FileID ProjectFile_;
};
    
}

#endif //PROJECT_H
