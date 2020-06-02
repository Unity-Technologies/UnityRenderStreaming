unityPath=$1
projectPath=$2
packagePath=$3
editorLogPath=$4

echo Exporting Package: ${packagePath}
${unityPath} -batchmode -nographics -quit -projectPath ${projectPath} -exportPackage "Assets" ${packagePath} -logFile ${editorLogPath}