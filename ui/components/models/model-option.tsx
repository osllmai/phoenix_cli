import { FC } from "react"
import { IconInfoCircle } from "@tabler/icons-react"
import { ModelIcon } from "./model-icon"
import { WithTooltip } from "../ui/with-tooltip"

export interface LLM {
  modelId: string
  modelName: string
  provider: string
  platformLink?: string
  imageInput: boolean
  hostedId?: number // Add hostedId if needed
}

interface ModelOptionProps {
  model: {
    modelId: string
    modelName: string
    provider: string
  }
  onSelect: () => void
}

export const ModelOption: FC<ModelOptionProps> = ({ model, onSelect }) => {
  return (
    <WithTooltip
      display={
        <div>
          {/* No pricing data is available from your API, so this section is removed */}
          {model.provider === "custom" && (
            <div className="space-y-1 text-sm">
              <div>
                <span className="font-semibold">Provider:</span> Custom Model
              </div>
            </div>
          )}
        </div>
      }
      side="bottom"
      trigger={
        <div
          className="hover:bg-accent flex w-full cursor-pointer justify-start space-x-3 truncate rounded p-2 hover:opacity-50"
          onClick={onSelect}
        >
          <div className="flex items-center space-x-2">
            <ModelIcon provider={model.provider} width={28} height={28} />
            <div className="text-sm font-semibold">{model.modelName}</div>
          </div>
        </div>
      }
    />
  )
}
