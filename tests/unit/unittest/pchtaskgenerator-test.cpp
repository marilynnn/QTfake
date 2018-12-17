/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "googletest.h"

#include "mockbuilddependenciesprovider.h"
#include "mockpchtasksmerger.h"

#include <pchtaskgenerator.h>

namespace {

using ClangBackEnd::BuildDependencies;
using ClangBackEnd::BuildDependency;
using ClangBackEnd::CompilerMacro;
using ClangBackEnd::FilePathId;
using ClangBackEnd::IncludeSearchPath;
using ClangBackEnd::IncludeSearchPathType;
using ClangBackEnd::PchTask;
using ClangBackEnd::PchTaskSet;
using ClangBackEnd::SourceEntries;
using ClangBackEnd::SourceType;
using ClangBackEnd::UsedMacro;
using ClangBackEnd::UsedMacros;

class PchTaskGenerator : public testing::Test
{
protected:
    NiceMock<MockBuildDependenciesProvider> mockBuildDependenciesProvider;
    NiceMock<MockPchTasksMerger> mockPchTaskMerger;
    ClangBackEnd::PchTaskGenerator generator{mockBuildDependenciesProvider, mockPchTaskMerger};
    ClangBackEnd::ProjectPartContainer projectPart1{
        "ProjectPart1",
        {"--yi"},
        {{"YI", "1", 1},
         {"QI", "7", 1},
         {"ER", "2", 2},
         {"SAN", "3", 3},
         {"SE", "4", 4},
         {"BA", "8", 4},
         {"WU", "5", 5},
         {"LUI", "6", 6},
         {"JIU", "9", 9},
         {"SHI", "10", 10}},
        {IncludeSearchPath{"/system/path", 2, IncludeSearchPathType::System},
         IncludeSearchPath{"/builtin/path", 3, IncludeSearchPathType::BuiltIn},
         IncludeSearchPath{"/framework/path", 1, IncludeSearchPathType::System}},
        {IncludeSearchPath{"/to/path1", 1, IncludeSearchPathType::User},
         IncludeSearchPath{"/to/path2", 2, IncludeSearchPathType::User}},
        {{1, 1}},
        {{1, 2}},
        Utils::Language::Cxx,
        Utils::LanguageVersion::CXX11,
        Utils::LanguageExtension::All};
    SourceEntries firstSources{{1, SourceType::ProjectInclude, 1},
                               {2, SourceType::UserInclude, 1},
                               {3, SourceType::TopProjectInclude, 1},
                               {4, SourceType::SystemInclude, 1},
                               {5, SourceType::TopSystemInclude, 1}};
    UsedMacros usedMacros{{"LIANG", 0},{"YI", 1}, {"ER", 2}, {"SAN", 3}, {"SE", 4}, {"WU", 5}};
    BuildDependency buildDependency{firstSources, usedMacros, {}, {}, {}};
};

TEST_F(PchTaskGenerator, AddProjectParts)
{
    ON_CALL(mockBuildDependenciesProvider, create(_)).WillByDefault(Return(buildDependency));

    EXPECT_CALL(
        mockPchTaskMerger,
        mergeTasks(
            ElementsAre(AllOf(
                Field(
                    &PchTaskSet::system,
                    AllOf(
                        Field(&PchTask::projectPartIds, ElementsAre("ProjectPart1")),
                        Field(&PchTask::includes, ElementsAre(5)),
                        Field(&PchTask::compilerMacros,
                              ElementsAre(CompilerMacro{"SE", "4", 4}, CompilerMacro{"WU", "5", 5})),
                        Field(&PchTask::usedMacros, ElementsAre(UsedMacro{"SE", 4}, UsedMacro{"WU", 5})),
                        Field(&PchTask::systemIncludeSearchPaths,
                              ElementsAre(
                                  IncludeSearchPath{"/system/path", 2, IncludeSearchPathType::System},
                                  IncludeSearchPath{"/builtin/path", 3, IncludeSearchPathType::BuiltIn},
                                  IncludeSearchPath{"/framework/path", 1, IncludeSearchPathType::System})),
                        Field(&PchTask::projectIncludeSearchPaths,
                              ElementsAre(IncludeSearchPath{"/to/path1", 1, IncludeSearchPathType::User},
                                          IncludeSearchPath{"/to/path2", 2, IncludeSearchPathType::User})),
                        Field(&PchTask::toolChainArguments, ElementsAre("--yi")),
                        Field(&PchTask::language, Eq(Utils::Language::Cxx)),
                        Field(&PchTask::languageVersion, Eq(Utils::LanguageVersion::CXX11)),
                        Field(&PchTask::languageExtension, Eq(Utils::LanguageExtension::All)))),
                AllOf(Field(
                    &PchTaskSet::project,
                    AllOf(
                        Field(&PchTask::projectPartIds, ElementsAre("ProjectPart1")),
                        Field(&PchTask::includes, ElementsAre(3)),
                        Field(&PchTask::compilerMacros,
                              ElementsAre(CompilerMacro{"YI", "1", 1}, CompilerMacro{"SAN", "3", 3})),
                        Field(&PchTask::usedMacros, ElementsAre(UsedMacro{"YI", 1}, UsedMacro{"SAN", 3})),
                        Field(&PchTask::systemIncludeSearchPaths,
                              ElementsAre(
                                  IncludeSearchPath{"/system/path", 2, IncludeSearchPathType::System},
                                  IncludeSearchPath{"/builtin/path", 3, IncludeSearchPathType::BuiltIn},
                                  IncludeSearchPath{"/framework/path", 1, IncludeSearchPathType::System})),
                        Field(&PchTask::projectIncludeSearchPaths,
                              ElementsAre(IncludeSearchPath{"/to/path1", 1, IncludeSearchPathType::User},
                                          IncludeSearchPath{"/to/path2", 2, IncludeSearchPathType::User})),
                        Field(&PchTask::toolChainArguments, ElementsAre("--yi")),
                        Field(&PchTask::language, Eq(Utils::Language::Cxx)),
                        Field(&PchTask::languageVersion, Eq(Utils::LanguageVersion::CXX11)),
                        Field(&PchTask::languageExtension, Eq(Utils::LanguageExtension::All))))))),
            ElementsAre(Eq("ToolChainArgument"))));

    generator.addProjectParts({projectPart1}, {"ToolChainArgument"});
}

TEST_F(PchTaskGenerator, RemoveProjectParts)
{
    ON_CALL(mockBuildDependenciesProvider, create(_)).WillByDefault(Return(buildDependency));

    EXPECT_CALL(
        mockPchTaskMerger,
        removePchTasks(ElementsAre("project1", "project2")));

    generator.removeProjectParts({"project1", "project2"});
}

}
