import shutil
import os

input_file = "./Data/Libs/GameTokens/GT_Global.xml"
output_files = [
    "./Data/Levels/Campaign/EndGame/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Engineering/CargoBay/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Engineering/LifeSupport/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Engineering/PowerSource/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Executive/Arboretum/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Executive/Bridge/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Executive/CorporateIT/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Executive/CrewFacilities/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Research/Lobby/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Research/Prototype/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Research/Psychotronics/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Research/ShuttleBay/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Research/SimulationLabs/level/gametokens/gt_global.xml",
    "./Data/Levels/Campaign/Station/Exterior/level/gametokens/gt_global.xml",
]

def copy_file_to_multiple_paths(input_file_path, output_file_paths):
    if not os.path.isfile(input_file_path):
        print(f"Error: The input file '{input_file_path}' does not exist.")
        return

    for output_path in output_file_paths:
        try:
            # make the directory if it doesn't exist
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            shutil.copy(input_file_path, output_path)
            print(f"Copied '{input_file_path}' to '{output_path}'")
        except Exception as e:
            print(f"Failed to copy to '{output_path}': {e}")

if __name__ == "__main__":
    copy_file_to_multiple_paths(input_file, output_files)