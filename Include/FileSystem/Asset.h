// =====================================================================================================================
// Copyright 2024 Medusa Slockbower
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =====================================================================================================================

#ifndef ASSET_H
#define ASSET_H

#include <open-cpp-utils/filesystem.h>

namespace ocu = open_cpp_utils;

namespace OpenShaderDesigner
{

class Asset
{
public:
    Asset() : Dirty_(false) { }
    
    bool Dirty() const { return Dirty_; }

    virtual void Open() = 0;
    virtual void Load(const std::filesystem::path& path) = 0;
    virtual void Save(const std::filesystem::path& path) { Dirty_ = false; };

protected:
    void MakeDirty() { Dirty_ = true; }

private:
    bool Dirty_;
};
    
}

#endif //ASSET_H
